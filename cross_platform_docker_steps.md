
1. If you haven't run this command:
```bash
$ docker run --privileged --rm docker/binfmt:a7996909642ee92942dcd6cff44b9b95f08dad64
```

2. Build
```bash
$ docker buildx build --platform linux/arm64/v8 -t <whatever> --push .
```

3. These might help if dpkg fails
```bash
$ docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
$ docker buildx rm builder
$ docker buildx create --name builder --driver docker-container --use
$ docker buildx inspect --bootstrap
```
