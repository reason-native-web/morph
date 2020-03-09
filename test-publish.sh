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
cp $name.json $name/package.json
package_name=$(extract name)
version=$(extract version)
npm_version=$(npm view "$package_name" version 2> /dev/null || echo 0)
if [ "$npm_version" != "$version" ]
then
    cp -r LICENSE $name/LICENSE
    cp -r README.md $name/README.md
    cp -r $name.opam $name/$name.opam
    cd $name
    # esy
    npm publish --access public --dry-run
    cd ..
    git clean -f -d ./$name
fi
done
