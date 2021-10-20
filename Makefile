.PHONY : build all build-vendor

build-vendor:
	echo 'build vendor and dependencies' \
	&& ./scripts/zlib \
	&& ./scripts/png \
	&& ./scripts/jpeg \
	&& ./scripts/webp \
	&& ./scripts/imagemagick \
	echo 'finish build vendor and dependencies'

build:
	echo 'clean and build project' \
	&& rm -rf build/ \
	&& npm run build \
	echo 'finish cleann and build project'