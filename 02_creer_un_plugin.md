# 02 — Créer un nouveau plugin
1. `cp -r skeleton monplugin`. Dans `monplugin/` : `project.conf` (`NAME`, `FILE`, `PUID` = 4 caractères en hexadécimal **uniques**, `LOGNAME`, `LOGUID` **différents**, `DEF`),
   renommer le `.def` (contenu : `EXPORTS` / `VSTPluginMain` / `main=VSTPluginMain`), adapter `version.rc`, puis `BF_NAME`, `BF_UID`, `BF_LOGFILE` dans `plugin.cpp`.
2. `plugin.cpp` définit avant `#include "vstbase.h"` : l'enum de paramètres (`P_COUNT`), `kNames` (<= 8 car.), `kEdNames`, `kUnits`, `kDefaults` (0..1) et la struct `Core` :
   `apply(const float* p)`, `fmt(int, char*) const`, `setSampleRate(float)`, `reset()`, `process(float** in, float** out, int n, unsigned now_ms)`,
   `meterName()`, `meter01()`, `meterText(char*)`.
3. Conversions 0..1 -> valeur : linéaire `lo+v*(hi-lo)` ; logarithmique `lo*exp(v*ln(hi/lo))` (`bf::mexp`) ; quadratique `max*v*v` ; zones `(int)(v*3.999f)`.
4. `./build.sh monplugin` et `./build.sh monplugin log`, puis `python3 tools/audit_dll.py bin64/MonPlugin_x64.dll`.
5. Ajouter un test (copier `tests/host_granulator.cpp` ou `host_skeleton.cpp`), le déclarer dans `tools/run_tests.sh`, lancer `./tools/run_tests.sh`.
6. Livrer `bin64/MonPlugin_x64.dll` + zip de `MonPlugin_x64_log.dll`.
Pour un delay/reverb/chorus : tampon `float` directement dans `Core`, indices masqués (puissance de 2), pas de `malloc`.
