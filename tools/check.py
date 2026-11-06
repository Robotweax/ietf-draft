"""Offline structural checks; intentionally not an RFCXML schema validator."""
from pathlib import Path
import re
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
drafts = sorted((root / "drafts").glob("*.xml"))
assert drafts, "No drafts found"
for path in drafts:
    doc = ET.parse(path).getroot()
    assert doc.tag == "rfc" and doc.get("version") == "3", path
    assert doc.get("docName") == path.stem, path
    anchors = [e.get("anchor") for e in doc.iter() if e.get("anchor")]
    assert len(anchors) == len(set(anchors)), f"Duplicate anchors: {path}"
    for xref in doc.iter("xref"):
        assert xref.get("target") in anchors, f"Broken xref: {path}"
    for anchor in ("status", "security", "iana", "issues"):
        assert anchor in anchors, f"Missing {anchor}: {path}"
    assert doc.find("front/abstract/t") is not None, path
    print(f"PASS structural XML: {path.relative_to(root)}")
for path in root.rglob("*.md"):
    for target in re.findall(r"\]\(([^)]+)\)", path.read_text()):
        if "://" in target or target.startswith("#"):
            continue
        assert (path.parent / target.split("#")[0]).exists(), (path, target)
print("PASS local Markdown link targets")
