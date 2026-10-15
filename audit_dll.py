#!/usr/bin/env python3
"""Audit d'une DLL VST2 64 bits. Usage : audit_dll.py a.dll [b.dll ...]
Verifie : format x86-64, exports (VSTPluginMain + main), imports limites a kernel32/user32/gdi32 (aucun runtime C),
pas de TLS, pas de symboles de debug, presence de la table d'exceptions (.pdata). Code de sortie 1 si un controle echoue."""
import sys
import pefile
ALLOWED = {"KERNEL32.DLL", "USER32.DLL", "GDI32.DLL"}
def audit(path):
    errs, warns = [], []
    pe = pefile.PE(path)
    if pe.FILE_HEADER.Machine != 0x8664: errs.append("pas du x86-64 (Machine=%#x)" % pe.FILE_HEADER.Machine)
    if not (pe.FILE_HEADER.Characteristics & 0x2000): errs.append("pas une DLL")
    exports = sorted(e.name.decode() for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name) if hasattr(pe, "DIRECTORY_ENTRY_EXPORT") else []
    if exports != ["VSTPluginMain", "main"]: errs.append("exports = %s (attendu : ['VSTPluginMain', 'main'])" % exports)
    imports = {}
    for d in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
        dll = d.dll.decode().upper(); imports[dll] = [i.name.decode() for i in d.imports if i.name]
        if dll not in ALLOWED: errs.append("DLL importee non autorisee : %s (runtime C ?)" % dll)
    secs = [s.Name.rstrip(b"\0").decode() for s in pe.sections]
    if any(s.startswith(".debug") for s in secs): warns.append("symboles de debug presents (utiliser -s)")
    if ".tls" in secs or pe.OPTIONAL_HEADER.DATA_DIRECTORY[9].Size: errs.append("TLS present")
    if ".pdata" not in secs: warns.append("pas de .pdata (table d'exceptions x64 absente)")
    if not (pe.OPTIONAL_HEADER.DllCharacteristics & 0x40): warns.append("ASLR (DYNAMIC_BASE) non active")
    return errs, warns, imports
ok = True
for p in sys.argv[1:]:
    errs, warns, imports = audit(p)
    print("[%s] %s%s" % ("OK " if not errs else "ECHEC", p.split("/")[-1], "   (" + ", ".join(sorted(imports)) + ")" if not errs else ""))
    for e in errs: print("      ERREUR  :", e)
    for w in warns: print("      attention:", w)
    ok &= not errs
sys.exit(0 if ok else 1)
