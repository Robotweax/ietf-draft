"""Independent KM/DATA calculation for six adapter cases; uses public fixture keys."""
import argparse,hashlib,json,struct
from pathlib import Path
from cryptography.hazmat.primitives.keywrap import aes_key_unwrap,aes_key_wrap
from cryptography.hazmat.primitives.ciphers import Cipher,algorithms,modes
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--write',action='store_true');args=p.parse_args()
km={};data={}
for line in (root/'evidence/adapter-matrix.log').read_text().splitlines():
 parts=line.split()
 if parts and parts[0]=='KM_CASE':km[(parts[1],int(parts[2]))]=bytes.fromhex(parts[3])
 if parts and parts[0]=='DATA_CASE':data[(parts[1],int(parts[2]))]=tuple(bytes.fromhex(s) for s in parts[3:])
expected={(mode,size) for mode in ['ctr','gcm'] for size in [16,24,32]}
assert set(km)==set(data)==expected
cases=[]
for mode,size in sorted(expected):
 body=km[(mode,size)];assert len(body)==40+size
 salt=body[16:32];assert salt==bytes(range(80,96))
 kek=hashlib.pbkdf2_hmac('sha1',b'conformance fixture password',salt[-8:],2048,size)
 key=aes_key_unwrap(kek,body[32:]);assert key==bytes(range(96,96+size));assert aes_key_wrap(kek,key)==body[32:]
 # Header contract: solo, in-order, even key, msgno7, sequence100.
 header=struct.pack('!IIII',100,0xe8000007,123456,0x05060708)
 clear,cipher,tag=data[(mode,size)]
 if mode=='gcm':
  iv=bytearray(salt[:12]);offset=8
 else:
  iv=bytearray(salt[:14]+bytes(2));offset=10
 for i,v in enumerate(struct.pack('!I',100)):iv[offset+i]^=v
 if mode=='gcm':
  protected=AESGCM(key).encrypt(bytes(iv),clear,header)
  assert protected==cipher+tag
  assert AESGCM(key).decrypt(bytes(iv),protected,header)==clear
 else:
  enc=Cipher(algorithms.AES(key),modes.CTR(bytes(iv))).encryptor()
  assert enc.update(clear)+enc.finalize()==cipher
  assert tag==bytes(16) # adapter placeholder, not a CTR wire tag
 cases.append(dict(mode=mode,key_bytes=size,km_body_hex=body.hex(),salt_hex=salt.hex(),kek_hex=kek.hex(),sek_hex=key.hex(),iv_hex=bytes(iv).hex(),header_hex=header.hex(),plaintext_hex=clear.hex(),ciphertext_hex=cipher.hex(),tag_hex=tag.hex() if mode=='gcm' else None))
fixture=dict(schema=1,provenance='C++ outputs independently recomputed with Python cryptography; deterministic public test material, not independent peer interoperability',cases=cases)
path=root/'vectors/crypto-matrix.json'
if args.write:path.write_text(json.dumps(fixture,indent=2)+'\n')
assert json.loads(path.read_text())==fixture,'frozen matrix mismatch'
print('PASS six cases: KEK derivation, RFC3394 wrap, CTR/GCM IV construction and exact ciphertext/tag')
