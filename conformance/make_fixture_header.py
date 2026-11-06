import json,sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
d=json.loads((root/'vectors/initial-wire-vectors.json').read_text())
values={k:d[k] for k in ['password_hex','synthetic_km_body_hex','fec_body_hex']}
for v in d['authentication']:
 name=v['name'].replace('-','_');values[name+'_tag']=v['tag_hex'];values[name+'_content']=v['content_hex']
for v in d['multicast']:values[v['name'].replace('-','_')]=v['extension_hex']
Path(sys.argv[1]).write_text('#pragma once\n#include <string_view>\n'+''.join(f'inline constexpr std::string_view {k}="{v}";\n' for k,v in values.items()))
