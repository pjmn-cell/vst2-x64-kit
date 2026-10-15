# Kit « plugins VST2 64 bits pour Windows »

Crée des effets VST2 **64 bits** (DLL autonome, sans runtime C), cross-compilés depuis Linux (MinGW-w64) et testés sous Wine.
Contient deux projets prêts à compiler : **Granulator** (effet granulaire) et **Skeleton** (gain d'exemple, point de départ).
Pour Windows 32 bits / Nuendo 2 / XP, voir l'autre kit (`vst2-xp-kit`) : ce kit-ci ne le remplace pas.

## Contenu
| Élément | Rôle |
|---|---|
| `build.sh` | `./build.sh <projet> [log]` ou `./build.sh all` -> `bin64/` |
| `common/` | `util.h`, `mathlite.h` (maths sans libm), `vst2_min.h` (en-tête VST2 réécrit), `vstbase.h` (base de plugin : VST, éditeur GDI, journal) |
| `skeleton/`, `granulator/` | `plugin.cpp`, `project.conf`, `.def`, `version.rc` |
| `tests/` | hôtes VST de test, exécutés sous Wine |
| `tools/` | `setup_env.sh`, `run_tests.sh`, `audit_dll.py` |
| `docs/` | 01 recette technique, 02 création d'un plugin, 03 diagnostic, 04 limites |
| `bin64/` | DLL déjà compilées (`*_x64.dll` et `*_x64_log.dll`) |
| `PROMPT_DEMARRAGE.md` | texte à coller dans une nouvelle conversation avec Claude |

## Démarrage rapide (Ubuntu/Debian, root pour l'installation)
    ./tools/setup_env.sh      # mingw-w64, wine, xvfb, pefile
    ./tools/run_tests.sh      # construit, audite, teste sous Wine -> "TOUS LES CONTROLES PASSENT"
    cp -r skeleton monplugin  # puis adapter (docs/02)
    ./build.sh monplugin

## Installer un plugin
Copier `Granulator_x64.dll` dans le dossier VST2 de l'hôte 64 bits (ex. `C:\Program Files\VSTPlugins`), puis rescanner.
En cas d'échec : essayer `Granulator_x64_log.dll` seule et lire `C:\granulator_log.txt`.

## Règles
1. Un seul PE x86-64, **aucun runtime C/C++** (`-nostdlib`), imports limités à kernel32/user32/gdi32 : `tools/audit_dll.py` le vérifie.
2. Exports **`VSTPluginMain` et `main`** (les hôtes 64 bits cherchent le premier, les anciens le second).
3. `effIdentify` (opcode 22) répond `'NvEf'` : inoffensif pour les hôtes modernes, exigé par certains anciens.
4. Livrer aussi la version `_log`, et dire clairement ce qui est testé (Wine) et ce qui ne l'est pas (DAW réels).
5. Nouveau plugin = nouveau nom de fichier, nom d'effet et `uniqueID` (4 caractères, uniques).
