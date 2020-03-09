set -e
packages=(
    morph
    morph_base
    morph_graphql_server
    morph_server_base
    morph_server_http
    morph_server_https
)
extract() {
# Field's key/value should be on their own line
echo $(cat package.json \
    | grep "$1" \
    | head -1 \
    | awk -F: '{ print $2 }' \
    | sed 's/[",]//g')
}

for name in "${packages[@]}"
do
mkdir -p publish
cp $name.json publish/package.json
package_name=$(extract name)
version=$(extract version)
npm_version=$(npm view "$package_name" version 2> /dev/null || echo 0)
if [ "$npm_version" != "$version" ]
then
    cp -r src/$name/ publish
    cp -r LICENSE publish
    cp -r README.md publish
    cp -r $name.opam publish
    cd publish
    esy
    npm publish --access public --dry-run
    cd ..
fi
rm -rf publish
done
