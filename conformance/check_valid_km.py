"""Cross-check the adapter's deterministic valid KM using independent byte parsing."""
import argparse,hashlib,json
from cryptography.hazmat.primitives.keywrap import aes_key_unwrap, aes_key_wrap
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--write',action='store_true');args=p.parse_args()
line=next(x for x in (root/'evidence/adapter.log').read_text().splitlines() if x.startswith('KM_BODY_HEX '))
body=bytes.fromhex(line.split()[1])
# This case is AES-128-CTR with one SEK: 16-byte KM header, salt16, wrapped24.
assert len(body)==56
salt=body[16:32];wrapped=body[32:]
assert salt==bytes(range(80,96))
password=b'conformance fixture password'
kek=hashlib.pbkdf2_hmac('sha1',password,salt[-8:],2048,16)
unwrapped=aes_key_unwrap(kek,wrapped)
assert unwrapped==bytes(range(96,112)), 'unexpected stream key'
rewrapped=aes_key_wrap(kek,unwrapped)
assert rewrapped==wrapped
fixture=dict(schema=1,provenance='Deterministic output from pinned Robotweax C++ adapter; RFC3394 wrap cross-checked with Python cryptography 41.0.3, not an independent peer',mode='AES-128-CTR',km_body_hex=body.hex(),password_hex=password.hex(),salt_hex=salt.hex(),kek_hex=kek.hex(),sek_hex=unwrapped.hex(),wrapped_key_hex=wrapped.hex())
path=root/'vectors/valid-km-transaction.json'
if args.write:path.write_text(json.dumps(fixture,indent=2)+'\n')
assert json.loads(path.read_text())==fixture,'frozen valid KM fixture mismatch'
print('PASS valid KM: exact frozen body, independent PBKDF2 input construction, RFC3394 unwrap/rewrap and expected SEK')
