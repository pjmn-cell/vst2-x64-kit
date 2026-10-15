# 03 — Diagnostic
## Si l'hôte ne charge pas le plugin
1. `python3 tools/audit_dll.py bin64/X.dll` doit afficher `[OK ]` (x86-64, exports `VSTPluginMain` + `main`, imports kernel32/user32/gdi32).
2. Vérifier que l'hôte est bien **64 bits** (un hôte 32 bits ne charge pas une DLL x64, et inversement) et que le dossier VST2 est scanné.
3. Essayer la version `_log` **seule** (nouveau nom de fichier, nom d'effet et uniqueID) : elle écrit `C:\<nom>_log.txt` (sinon `%TEMP%`).
   - aucun fichier : DLL jamais chargée (dossier non scanné, plugin sur liste noire, mauvaise architecture).
   - `DLL_PROCESS_ATTACH` seul : `main` non appelé ou en échec.
   - journal arrêté après `op=N -> r` : l'hôte a refusé la réponse à cet opcode.
4. Comparer avec un plugin qui marche : `host_generic.exe` affiche tous les champs d'`AEffect` et les réponses aux opcodes 3 à 80.
## Wine (rappels)
- `WINEDEBUG=-all WINEPREFIX=/tmp/wp64 WINEARCH=win64 wine host_xxx.exe plugin.dll` ; le lanceur s'appelle `wine` (pas `wine64`).
- Fenêtres : `xvfb-run -a -s "-screen 0 800x600x24" wine ...` ; toujours préfixer d'un `timeout`.
- Sorties en `\r\n` : `tr -d '\r'`. `FPUTRAP=1` démasque invalid / div-par-zéro / overflow SSE (MXCSR 0x0680).
- Les tests Wine prouvent que la DLL est valide et que DSP/éditeur marchent ; ils ne prouvent pas qu'un DAW précis l'acceptera.
