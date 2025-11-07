# Decoding and reconstruction of Banco

## Decoding

For banco, decoding is just a reformatting step, from the native root files to an event based TTree.
One has to run `decode_banco` for each ladder separately.
The decoder takes as input the data file and the noise data file.

```
./decode_banco ladder.root noise.root
```

A convenient way to run all togher is this bash command

```bash
for a in *root; do n=$( echo $a | cut -d- -f3); echo $n; PATH/TO/decode_banco $a PATH/TO/NOISE/multinoiseScan_XXXXXXXXXX_NOBEAM-B0-$n ; mv fdec.root $n; done
```


## Reconstruction

`reco_banco` comes with an helper. Run it with -h to print it.

At least, it needs the four ladder decoded root files.

If the geometry files are not in the local directory, then you can specify the directory with the option `-d [path]`. For 2025, the geometry is stored in this repository under `geometrySPS2025`
