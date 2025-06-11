# ensure login is stored
#echo $CR_PAT | docker login ghcr.io -u USERNAME --password-stdin

for i in  "debian:bookworm" "ubuntu:focal" "ubuntu:jammy" "ubuntu:noble";
do 
	podman build --build-arg base_container=$i -t ghcr.io/johanvdw/saga-watem-build-$i .
	podman push ghcr.io/johanvdw/saga-watem-build-$i
done;

podman build -f Dockerfile-docs -t ghcr.io/johanvdw/saga-watem-build-docs
podman push ghcr.io/johanvdw/saga-watem-build-docs
