{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    mach-nix = {
      url = "github:DavHau/mach-nix";
      inputs.pypi-deps-db = {
        url = "github:DavHau/pypi-deps-db/41546bb3c40dd1750d28ff5f49c3d68a8f11251b";
      };
    };
  };

  outputs = { nixpkgs, flake-utils, mach-nix, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            platformio
            nodejs
            (mach-nix.lib."${system}".mkPython {
              python = "python310";
              requirements = builtins.readFile ./webserver/requirements.txt;
            })
            mongosh
          ];
        };
      }
    );
}
