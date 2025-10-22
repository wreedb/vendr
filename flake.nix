{
  description = "Easily vendor your projects' dependencies";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "vendr";
          version = "0.2.1";
          src = ./.;

          meta = {
            description = "Easily vendor your projects' dependencies";
            longDescription = ''
              *Vendr* allows you to declare your dependencies in a TOML
              file format; It is meant to supplement where you would
              otherwise use Git submodules or direct source vendoring.
              It supports downloading files over http[s] and shallow
              fetching Git repositories.
            '';
            homepage = "https://github.com/wreedb/vendr";
            license = pkgs.lib.licenses.gpl3Plus;
          };

          nativeBuildInputs = with pkgs; [
            pkg-config
            meson
            ninja
            cmake
            scdoc
          ];

          buildInputs = with pkgs; [
            tomlplusplus
            libgit2
            cpr
            curl
          ];

          mesonBuildType = "release";
          mesonFlags = [
            # toml.hpp is vendored by default, though in a flake this is unnecessary.
            "-D tomlpp-header-only=false"
            # normally built static from cpr.wrap, doesn't apply here.
            "-D static-libcpr=false"
          ];

        };

        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            pkg-config
            meson
            ninja
            cmake
            scdoc
          ];

          buildInputs = with pkgs; [
            tomlplusplus
            libgit2
            cpr
            curl
          ];
        };
      });
}
