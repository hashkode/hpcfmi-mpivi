max=$1

cd "$(dirname "$0")"
cd ../build

for i in `seq 1 $max`
do
    mpirun -np 2 ./mpi-vi -d
done
