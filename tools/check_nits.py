"""Run local idnits against XML and rendered text; remote lookups opt-in."""
import argparse, concurrent.futures, hashlib, json, subprocess
from pathlib import Path
parser=argparse.ArgumentParser()
parser.add_argument('--idnits',required=True)
parser.add_argument('--online',action='store_true',help='Allow reference metadata lookups, not document submission')
parser.add_argument('--evidence-dir', default='evidence', help='Repository-relative output directory; use a release-specific directory to preserve old results')
args=parser.parse_args()
root=Path(__file__).resolve().parents[1]
evidence=(root/args.evidence_dir).resolve()
if root not in evidence.parents:
    parser.error('--evidence-dir must be inside the repository')
evidence.mkdir(parents=True, exist_ok=True)
label='online' if args.online else 'offline'
version=subprocess.check_output([args.idnits,'--version'],text=True).strip()
def run(item):
    topic,ext,folder,mode=item
    path=root/f'{folder}/draft-robotweax-srt-{topic}-00.{ext}'
    command=[args.idnits,'--mode',mode,'--fail-on','errors','--output','json','--remote-timeout','10']
    if not args.online:command.append('--offline')
    command.append(str(path))
    result=subprocess.run(command,capture_output=True,text=True,timeout=90)
    log=f'idnits-final-{label}-{topic}-{ext}-{mode}.json'
    (evidence/log).write_text(result.stdout)
    (evidence/log.replace('.json','.stderr')).write_text(result.stderr)
    parsed=json.loads(result.stdout)
    return dict(log=log,exit_code=result.returncode,input=str(path.relative_to(root)),input_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),mode=mode,offline=not args.online,counts=parsed['nitsBySeverity'])
items=[(topic,ext,folder,mode) for topic in sorted(path.stem.removeprefix('draft-robotweax-srt-').removesuffix('-00') for path in (root/'drafts').glob('draft-robotweax-srt-*-00.xml')) for ext,folder in [('xml','drafts'),('txt','build')] for mode in ('submission','normal')]
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:runs=list(pool.map(run,items))
(evidence/f'idnits-{label}-summary.json').write_text(json.dumps(dict(tool='@ietf-tools/idnits',version=version,runs=runs),indent=2)+'\n')
for item in runs:print(item['input'],item['mode'],item['counts'])
raise SystemExit(1 if any(item['exit_code'] for item in runs) else 0)
