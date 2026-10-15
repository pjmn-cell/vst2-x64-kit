#!/bin/bash
# Installe tout ce qu'il faut (Ubuntu/Debian, en root). A lancer une fois dans un nouvel environnement.
set -e
apt-get update -qq
apt-get install -y -qq g++-mingw-w64-x86-64 wine wine64 xvfb python3-pip
pip install pefile pillow --break-system-packages -q
echo "OK. Premier lancement de wine : WINEARCH=win64 WINEPREFIX=/tmp/wp64 wineboot (ou simplement lancer les tests)."
