{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    cmake
    ninja
    gtest

    # compile time profiling
    clang
    ungoogled-chromium
  ];
}
