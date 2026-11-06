"""Independent formula-level SRT fixtures. Does not load or call Robotweax code."""
import argparse, hashlib, hmac, ipaddress, json, struct, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
FIXTURE=ROOT/'vectors/initial-wire-vectors.json'

def fixtures():
    label=b'RWSRT-SESSION-2\0'
    assert len(label)==16
    caller=bytes(range(32)); listener=bytes(range(32,64))
    # Public, artificial test input, deliberately containing an embedded NUL.
    password=b'fixture\0password'
    salt=label+caller+listener+struct.pack('!QQ',0x01020304,0x05060708)
    key=hashlib.pbkdf2_hmac('sha1',password,salt,2048,32)
    # Deliberately synthetic opaque body: not semantically valid SRT KM.
    body=bytes.fromhex('000102030405060708090a0b0c0d0e0f')
    macs=[]
    for name,domain,role,counter in [('listener-proof',1,2,0),('caller-proof',1,1,0),('caller-kmreq',2,1,1),('listener-kmrsp',3,2,1)]:
        message=label+struct.pack('!BBQH',domain,role,counter,len(body))+body
        tag=hmac.digest(key,message,'sha256')
        wire=struct.pack('!I',2)+caller+listener+tag if domain==1 else struct.pack('!Q',counter)+body+tag
        macs.append(dict(name=name,domain=domain,role=role,counter=counter,mac_input_hex=message.hex(),tag_hex=tag.hex(),content_hex=wire.hex()))
    # Complete DATA header: seq 9, solo boundary, msgno zero, timestamp 1234,
    # destination socket 0x05060708. No retransmission or key-selector bits.
    header=struct.pack('!IIII',9,0xc0000000,1234,0x05060708)
    parity=bytes.fromhex('deadbeef01020304')
    fec_input=label+struct.pack('!BBQH',4,1,1,len(parity))+header+parity
    fec_tag=hmac.digest(key,fec_input,'sha256')
    macs.append(dict(name='caller-fec',domain=4,role=1,counter=1,mac_input_hex=fec_input.hex(),tag_hex=fec_tag.hex(),content_hex=(struct.pack('!Q',1)+parity+fec_tag).hex()))
    multicast=[]
    for family,address,iface in [(4,'239.255.133.11',ipaddress.ip_address('192.0.2.1').packed+bytes(12)),(6,'ff15::1234',bytes(12)+struct.pack('!I',7))]:
        addr=ipaddress.ip_address(address).packed
        if family==4:addr+=bytes(12)
        content=struct.pack('!IIBBH',0x52574d01,0x40000042,family,1,10000)+addr+iface+struct.pack('!IqB',0x7ffffffd,0,1)+bytes(3)
        assert len(content)==60
        wire=struct.pack('!HH',0x7f14,15)+content
        multicast.append(dict(name=f'ipv{family}-dispatch',extension_hex=wire.hex(),content_bytes=60,word_count=15,delta=0))
    return dict(schema=1,scope='Independent formula-level fixtures, not implementation conformance or valid KM transactions',password_hex=password.hex(),salt_hex=salt.hex(),iterations=2048,derived_key_hex=key.hex(),synthetic_km_body_hex=body.hex(),fec_header_hex=header.hex(),fec_body_hex=parity.hex(),authentication=macs,multicast=multicast)

def openssl_check(data,exe):
    # Every argument is artificial public fixture data. Never accepts user secrets.
    out=subprocess.check_output([exe,'kdf','-keylen','32','-kdfopt','digest:SHA1','-kdfopt','hexpass:'+data['password_hex'],'-kdfopt','hexsalt:'+data['salt_hex'],'-kdfopt','iter:2048','PBKDF2'],text=True)
    actual=''.join(out.strip().split(':')).lower()
    if actual!=data['derived_key_hex']:raise SystemExit('OpenSSL PBKDF2 mismatch')
    for case in data['authentication']:
        out=subprocess.check_output([exe,'mac','-digest','SHA256','-macopt','hexkey:'+data['derived_key_hex'],'HMAC'],input=bytes.fromhex(case['mac_input_hex']))
        if out.decode().strip().lower()!=case['tag_hex']:raise SystemExit('OpenSSL HMAC mismatch: '+case['name'])
    print('PASS OpenSSL CLI cross-check: PBKDF2 and five HMAC inputs (not an independent protocol implementation)')

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--write',action='store_true');parser.add_argument('--openssl');args=parser.parse_args()
    data=fixtures()
    if args.write:FIXTURE.write_text(json.dumps(data,indent=2)+'\n')
    if json.loads(FIXTURE.read_text())!=data:raise SystemExit('Frozen fixture mismatch; investigate before --write')
    # Sanity assertions on layout, domain separation and selected byte positions.
    assert len(bytes.fromhex(data['salt_hex']))==96
    assert len({v['tag_hex'] for v in data['authentication']})==5
    for case in data['multicast']:
        wire=bytes.fromhex(case['extension_hex'])
        assert len(wire)==64 and struct.unpack('!HH',wire[:4])==(0x7f14,15)
        assert wire[52:60]==bytes(8) and wire[61:64]==bytes(3)
    print('PASS frozen vectors: two multicast records, two proofs, KMREQ, KMRSP and FEC MAC')
    if args.openssl:openssl_check(data,args.openssl)
if __name__=='__main__':main()
