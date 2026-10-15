# Texte à coller dans une nouvelle conversation (avec le kit joint)
---
Voici mon kit `vst2-x64-kit` (archive jointe). Décompresse-le, lis `README.md`, `docs/01_recette_technique.md` et
`docs/02_creer_un_plugin.md`, puis fais-moi un nouveau plugin VST2 **64 bits** :

**Plugin souhaité :** *(décris l'effet)*
**Paramètres voulus :** *(liste, ou « à toi de proposer »)*

Contraintes (déjà dans le kit) : DLL x64 sans runtime C, éditeur à curseurs, sans MIDI ; livrer la DLL et la version `_log`
(dans un zip) après `./tools/run_tests.sh` sous Wine. Réponds en français, brièvement.
---
## Notes pour Claude
- Aucun souvenir d'une conversation à l'autre : tout est dans ce kit. Lancer `tools/setup_env.sh` puis `tools/run_tests.sh`.
- Appeler Wine par `wine` (jamais `wine64`, absent du PATH) avec `WINEARCH=win64`, et `timeout` sur chaque exécution ; ne pas utiliser `pkill -f` avec un motif présent dans sa propre commande.
- Copier `skeleton/` ou `granulator/` ; ne pas repartir de zéro.
