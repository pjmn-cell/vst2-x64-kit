# 04 — Limites et légal
## Limites
- Stéréo 2/2, pas de MIDI/instruments, pas de presets/chunks, pas de `processDoubleReplacing`, pas de vrai side-chain (réservé au VST3).
- Éditeur minimal GDI (curseurs). Hôtes 64 bits uniquement pour les DLL x64 ; pas de version Mac/Linux.
- Testé sous Wine seulement : aucun DAW réel (Cubase, Reaper, FL Studio...) n'a été essayé ici.
- VST2 n'est plus maintenu : certains hôtes récents ne le chargent plus.
## Légal
- Le SDK VST2 de Steinberg n'est plus distribué ; `common/vst2_min.h` est un en-tête **réécrit** (ABI seulement).
  Usage personnel : sans souci pratique ; distribution publique d'un VST2 : soumise à la licence Steinberg (plus délivrée aux nouveaux développeurs).
