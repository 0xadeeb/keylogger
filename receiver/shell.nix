{ pkgs ? import <nixpkgs> {} }:

with pkgs;

mkShell {
  buildInputs = [
    boost
    clang-tools
    cmake
    cmake-language-server
    gcc
    pkgconf
  ];
}
