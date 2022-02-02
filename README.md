# fha-parallelprogrammierung

```bash
clang -o example_openmp -fopenmp=libomp example_openmp.c -lm
```

```bash
mpicc -o example_mpi example_mpi.c
```

## Building
```bash
docker build . -t pblan/fha-parallelprogrammierung
``` 

## ToDo

- [] Adding `root` support for `mpiexec`
```bash
You can override this protection by adding the --allow-run-as-root option
to the cmd line or by setting two environment variables in the following way:
the variable OMPI_ALLOW_RUN_AS_ROOT=1 to indicate the desire to override this
protection, and OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1 to confirm the choice and
add one more layer of certainty that you want to do so.
We reiterate our advice against doing so - please proceed at your own risk.
```