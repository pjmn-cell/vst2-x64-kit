# 01 — Recette technique (x64)
## ABI VST2 64 bits
`AEffect` fait **192 octets** (144 en 32 bits) : pointeurs et `intptr_t` sur 8 octets. `vst2_min.h` le vérifie par `static_assert`.
Le callback hôte et le `dispatcher` utilisent `intptr_t` pour `value` et le retour. Sous x64 il n'y a qu'une convention d'appel.
Paramètres = floats 0..1 ; l'hôte lit les textes via `effGetParamDisplay` (9 caractères max, NUL compris).
`ERect` = `{top,left,bottom,right}` en `short`.
## Opcodes gérés (`vstbase.h`)
open 0, close 1 (libère, renvoie 1), getProgram 3, getProgramName 5, getParamLabel/Display/Name 6/7/8, setSampleRate 10, mainsChanged 12,
editGetRect/Open/Close/Idle 13/14/15/19, **effIdentify 22 -> 'NvEf'**, canBeAutomated 26, getPlugCategory 35, nom/vendeur/produit 45/47/48,
vendorVersion 49 (1000), vstVersion 58 (2300). Le reste renvoie 0.
## Compilation (`build.sh`)
| Option | Raison |
|---|---|
| `-nostdlib -lkernel32 -luser32 -lgdi32 -lgcc` | aucun runtime ; `-lgcc` fournit `___chkstk_ms` |
| `-Wl,-e,DllMain` | point d'entrée x64 (sans décoration) ; `DllMain` maison renvoie TRUE |
| `-msse2 -mfpmath=sse` | flottants en SSE (obligatoire en x64) |
| `-fno-tree-loop-distribute-patterns` | sinon GCC génère des appels `memset`/`memcpy` (fournis dans `vstbase.h`) |
| `-fno-math-errno` | `sqrtf` devient `sqrtss`, sans libm |
| `-fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-stack-protector` | rien à l'exécution |
| `-s` | pas de symboles de debug |
| `--dynamicbase --nxcompat`, `--subsystem windows:5.2` | ASLR et DEP activés |
| (unwind tables conservées) | la table `.pdata` x64 reste utile aux hôtes (débogueurs, gestion de plantages) |
| `.def` : `VSTPluginMain` + `main=VSTPluginMain` | les deux noms d'export ; pas de `__declspec(dllexport)` |
## Règles de code (pas de runtime)
- Pas de libm : `bf::mexp`, `bf::mlog`, `bf::msincos` (angle dans [0, pi]), `bf::msqrt`, `bf::mfabs`.
- Pas de printf/std:: : `fmtNum`, `put`, `cat`, `slen` (`util.h`). Pas de new/malloc : `bfAlloc`/`bfFree`.
- L'instance (`Plugin`) est allouée **à zéro** sur le tas : de gros tampons dans `Core` sont possibles (Granulator : 2 Mo).
- Pas d'objet global avec constructeur, pas de `thread_local`. Tableaux locaux < 4 Ko.
- Ajouter `1e-18f` aux entrées qui nourrissent des états décroissants (dénormaux).
- Stéréo 2 in / 2 out ; blocs de taille quelconque ; `process` doit accepter `in == out`.
## Éditeur (`vstbase.h`)
Fenêtre enfant 300 x (6 + (N+1)*30 + 6) px, N curseurs + 1 afficheur, double tampon GDI ; `SetWindowLongPtrA` (x64 correct).
