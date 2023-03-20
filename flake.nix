{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    mach-nix.url = "github:DavHau/mach-nix";
  };

  outputs = { nixpkgs, flake-utils, mach-nix, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        pythonPackages = p: with p; [ pyserial ];
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            platformio
            (mach-nix.lib."${system}".mkPython {
              requirements = builtins.readFile ./bridge/requirements.txt;
            })
          ];
        };
      }
    );
}
