#!/bin/bash
# Compile des plugins VST2 64 bits pour Windows (sans runtime C, SSE2, DLL autonome). Sortie : bin64/
# Usage : ./build.sh <dossier-projet> [clean|log]     ex. ./build.sh granulator log
#         ./build.sh all                              (tous les projets, versions normale ET journal)
#   clean = version normale ; log = version qui écrit un journal (C:\<nom>_log.txt, sinon %TEMP%) pour diagnostiquer.
set -e
ROOT="$(cd "$(dirname "$0")" && pwd)"; mkdir -p "$ROOT/bin64"
build_one() {
  local PRJ=$1 MODE=${2:-clean}
  ( cd "$ROOT/$PRJ"; source ./project.conf
    local EXTRA="" OUTN="${FILE}_x64" N="$NAME" U="$PUID"
    if [ "$MODE" = log ]; then EXTRA="-DBF_LOG"; OUTN="${FILE}_x64_log"; N="$LOGNAME"; U="$LOGUID"; fi
    local BASE="-O2 -fno-tree-loop-distribute-patterns -std=gnu++17 -DBF_NOCRT -DBF_EDITOR -fno-exceptions -fno-rtti -fno-math-errno -fno-stack-protector -fno-threadsafe-statics -shared -nostdlib -s"
    local LD="-Wl,-e,DllMain -Wl,--subsystem,windows:5.2 -Wl,--dynamicbase -Wl,--nxcompat -Wl,--file-alignment,0x200 -Wl,--section-alignment,0x1000"
    x86_64-w64-mingw32-windres -O coff version.rc -o ver.o
    x86_64-w64-mingw32-g++ -I"$ROOT/common" -I. $BASE $EXTRA "-DBF_NAME=\"$N\"" -DBF_UID=$U -msse2 -mfpmath=sse $LD -o "$ROOT/bin64/$OUTN.dll" plugin.cpp ver.o "$DEF" -lkernel32 -luser32 -lgdi32 -lgcc
    rm -f ver.o; echo "OK -> bin64/$OUTN.dll" )
}
if [ "${1:-}" = all ]; then
  for d in "$ROOT"/*/; do
    if [ -f "$d/project.conf" ]; then for m in clean log; do build_one "$(basename "$d")" $m; done; fi
  done
else build_one "${1:?usage: ./build.sh <projet> [clean|log] | all}" "${2:-clean}"; fi
