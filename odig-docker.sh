set -e

docker build --rm -f "Dockerfile.odig" -t morph-docs:latest "."

rm -rf _docs

docker cp --follow-link \
        $(docker create morph-docs:latest):/home/opam/opam-repository/_opam/var/cache/odig/html/ \
        ./_docs

find _docs -type l -delete

find _docs -type f -name *.html -exec sed -i '' -e 's/_odoc-theme/odoc-theme/g' {} +;
mv _docs/_odoc-theme _docs/odoc-theme

cp docs/index.html _docs/index.html

set -e

printf "Deploy to GitHub pages? [Yn] "
read action
if [ "x$action" == "xn" ]; then exit 2; fi
if [ "x$action" == "xN" ]; then exit 2; fi

curr_branch=$(git rev-parse --abbrev-ref HEAD)

git checkout gh-pages
git pull

find _docs -type d -depth 1 | sed 's|^_docs/||' | xargs -L1 rm -rf

mv _docs/* ./

git add .

printf "Enter your commit message: "
read commit_message

git commit -m "$commit_message"
git push

printf "Go back to last branch? [Yn] "
read action_two
if [ "x$action_two" == "xn" ]; then echo $curr_branch; exit 2; fi
if [ "x$action_two" == "xN" ]; then echo $curr_branch; exit 2; fi
git checkout $curr_branch
