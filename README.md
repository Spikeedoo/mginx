# mginx
Simple C++ Web Server


## Compile
To compile the binary, change into the `src` directory and type:
```
make
```

## Configure
To configure the server, edit the `config.yaml` file in the base directory.
```yaml
basedir: # Directory to serve
```

## Run
After compiling, the binary will be at `bin/mginx`
```
./mginx <port>
