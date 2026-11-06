"""Verify pinned evidence references, not protocol behavior. No network access."""
import argparse, hashlib, json, re, subprocess
from pathlib import Path
parser=argparse.ArgumentParser()
parser.add_argument('--source-repo', required=True)
args=parser.parse_args()
root=Path(__file__).resolve().parents[1]
audit=json.loads((root/'audit/source-map.json').read_text())
commit=audit['implementation_commit']
cache={}
def read(path):
    if path not in cache:
        cache[path]=subprocess.check_output(['git','-C',args.source_repo,'show',f'{commit}:{path}'])
    return cache[path]
ids=set()
for claim in audit['claims']:
    assert claim['id'] not in ids
    ids.add(claim['id'])
    source=claim['source']
    data=read(source['path']); text=data.decode()
    assert hashlib.sha256(data).hexdigest()==source['sha256'], claim['id']
    assert text[:text.index(source['needle'])].count('\n')+1==source['line'], claim['id']
    for test in claim['tests']:
        data=read(test['path']);text=data.decode()
        assert hashlib.sha256(data).hexdigest()==test['sha256'], test['name']
        match=re.search(r'TEST\(\s*'+re.escape(test['name'])+r'\s*\)',text)
        assert match and text[:match.start()].count('\n')+1==test['line'], test['name']
        assert test['execution'] in ('not-run', 'passed')
        if test['execution']=='passed':
            log=(root/test['evidence_log']).read_text()
            assert '[PASS] '+test['name'] in log, test['name']
print(f"PASS: {len(ids)} claim anchors and their test references against {commit}; execution evidence checked where recorded")

manifest_path=root/'evidence/run-manifest.json'
if manifest_path.exists():
    manifest=json.loads(manifest_path.read_text())
    assert manifest['source_commit']==commit
    for run in manifest['runs']:
        log=root/'evidence'/run['log']
        assert hashlib.sha256(log.read_bytes()).hexdigest()==run['sha256'], run['log']
    for path, expected in manifest['fixture_files'].items():
        assert hashlib.sha256((root/path).read_bytes()).hexdigest()==expected, path
    assert hashlib.sha256((root/'conformance/adapter.cpp').read_bytes()).hexdigest()==manifest['adapter_source_sha256']
    print('PASS execution-log, fixture and adapter-source hashes')

    for path, expected in manifest.get('current_adapter_sources', {}).items():
        assert hashlib.sha256((root/path).read_bytes()).hexdigest()==expected, path
    print('PASS current matrix-adapter source hashes')

    for path, expected in manifest.get('native_instrumentation', {}).get('files', {}).items():
        assert hashlib.sha256((root/path).read_bytes()).hexdigest()==expected, path
    print('PASS native instrumentation source/patch hashes')

baseline_manifest=root/'audit/baselines/manifest.json'
if baseline_manifest.exists():
    baseline=json.loads(baseline_manifest.read_text())
    for item in baseline['files']:
        data=(root/item['path']).read_bytes()
        assert len(data)==item['bytes'], item['path']
        assert hashlib.sha256(data).hexdigest()==item['sha256'], item['path']
    data=(root/'audit/baselines/haivision-461a1fea.md.txt').read_bytes()
    assert hashlib.sha1(b'blob '+str(len(data)).encode()+b'\0'+data).hexdigest()==baseline['supplemental_git_blob']
    old=(root/'audit/baselines/draft-sharabayko-srt-01.txt').read_text().split('\n')
    new=data.decode().split('\n')
    rows=json.loads((root/'audit/baselines/submitted-sections.json').read_text())
    normalize=lambda value: re.sub(r'[^a-z0-9]', '', value.lower())
    section_ids=set()
    for row in rows:
        assert row['section'] not in section_ids
        section_ids.add(row['section'])
        assert old[row['line']-1].endswith(row['title']), row['section']
        heading=re.sub(r'\s*\{#[^}]+\}$', '', new[row['supplemental_line']-1])
        assert normalize(heading)==normalize(row['title']), row['section']
    expected=sum(bool(re.match(r'^((?:\d+|[A-C])(?:\.\d+)*[.]?)  +(.+)$', line) or re.match(r'^Appendix [A-C]\.  ', line)) for line in old)
    assert len(rows)==expected
    print(f'PASS baseline hashes, supplemental Git blob and {len(rows)} section mappings')

clause_path=root/'audit/proposed-clauses.json'
if clause_path.exists():
    clauses=json.loads(clause_path.read_text())
    assert clauses['implementation_commit']==commit
    document=(root/clauses['document']).read_text()
    seen=set()
    for clause in clauses['clauses']:
        assert clause['id'] not in seen
        seen.add(clause['id'])
        assert f"**{clause['id']} " in document
        assert clause['status']=='proposed-not-approved'
        for source in clause['sources']:
            data=read(source['path']) if source['kind']=='implementation' else (root/source['path']).read_bytes()
            assert hashlib.sha256(data).hexdigest()==source['sha256'], clause['id']
            text=data.decode()
            assert text[:text.index(source['needle'])].count('\n')+1==source['line'], clause['id']
    print(f'PASS {len(seen)} proposed-clause source mappings (not approval or conformance)')
