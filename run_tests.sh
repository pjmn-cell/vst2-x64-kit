#!/bin/bash
# Construit tout, audite les DLL, puis execute les hotes de test SOUS WINE 64 bits (vraies DLL Windows x64).
# Affiche PASS / FAIL pour chaque controle. A lancer AVANT de livrer un plugin.
ROOT="$(cd "$(dirname "$0")/.." && pwd)"; cd "$ROOT"
export WINEDEBUG=-all WINEPREFIX=${WINEPREFIX:-/tmp/wp64} WINEARCH=win64
W=/tmp/kit64_tests; mkdir -p $W; FAIL=0
res() { if [ "$1" = 0 ]; then echo "  PASS  $2"; else echo "  FAIL  $2"; FAIL=1; fi; }
echo "== 1) compilation de tous les projets"; ./build.sh all > $W/build.log 2>&1; res $? "build.sh all"
echo "== 2) audit des DLL (format x64, exports, imports, TLS...)"; python3 tools/audit_dll.py bin64/*.dll; res $? "audit_dll.py"
echo "== 3) compilation des hotes de test (x64)"
for t in host_generic host_identify host_skeleton host_granulator; do
  x86_64-w64-mingw32-g++ -O1 -std=gnu++17 -I common -o $W/$t.exe tests/$t.cpp -static -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lkernel32 2>$W/$t.err; res $? "$t.exe"; done
cp bin64/*.dll $W/
echo "== 4) effIdentify = 'NvEf' pour chaque DLL (hotes anciens)"
n=$(ls bin64/*.dll | wc -l); ok=$(cd $W && timeout 120 wine host_identify.exe $(cd "$ROOT/bin64" && ls *.dll) 2>&1 | grep -ac "'NvEf'")
[ "$ok" -ge "$n" ]; res $? "$ok/$n DLL repondent 'NvEf'"
echo "== 5) Skeleton (gain +/-12 dB, editeur)"
out=$(cd $W && timeout 150 xvfb-run -a -s "-screen 0 800x600x24" wine host_skeleton.exe Skeleton_x64.dll 2>&1 | tr -d "\r"); echo "$out" | grep -a "param="
echo "$out" | grep -aq "^PASS"; res $? "skeleton : gain exact et editeur"
echo "== 6) Granulator (dry exact, niveau, pitch +/-12, extremes, silence, editeur) - SSE strict"
for d in Granulator_x64.dll Granulator_x64_log.dll; do
  out=$(cd $W && timeout 150 xvfb-run -a -s "-screen 0 800x600x24" env FPUTRAP=1 wine host_granulator.exe $d 2>&1 | tr -d "\r"); echo "$out" | grep -a "dry:\|wet=\|pitch"
  echo "$out" | grep -aq "^PASS"; res $? "$d : tous les controles"; done
echo "== 7) hote generique : sequence d'init, opcodes, processReplacing"
out=$(cd $W && timeout 120 wine host_generic.exe Granulator_x64.dll 2>&1 | tr -d "\r"); echo "$out" | grep -aq "FIN OK"; res $? "host_generic : sequence complete"
echo; if [ $FAIL = 0 ]; then echo "TOUS LES CONTROLES PASSENT"; else echo "AU MOINS UN CONTROLE ECHOUE (voir ci-dessus, journaux dans $W)"; fi
exit $FAIL
