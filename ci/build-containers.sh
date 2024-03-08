for i in "debian:bookworm";
do 
	podman build --build-arg base_container=$i -t ghcr.io/johanvdw/saga-watem-build-$i .
	podman push ghcr.io/johanvdw/saga-watem-build-$i
done;

podman build -f Dockerfile-docs -t ghcr.io/johanvdw/saga-watem-build-docs
podman push ghcr.io/johanvdw/saga-watem-build-docs
