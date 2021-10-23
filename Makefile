.PHONY : build all build-vendor

build-vendor:
	echo 'build vendor and dependencies...' \
	&& chmod +x ./scripts/zlib && chmod +x ./scripts/png && chmod +x ./scripts/jpeg && chmod +x ./scripts/webp && chmod +x ./scripts/imagemagick \
	&& ./scripts/zlib \
	&& ./scripts/png \
	&& ./scripts/jpeg \
	&& ./scripts/webp \
	&& ./scripts/imagemagick \
	&& echo 'finish build vendor and dependencies...'

build:
	echo 'clean and build project...' \
	&& rm -rf build/ \
	&& npm run build \
	&& echo 'finish clean and build project...'