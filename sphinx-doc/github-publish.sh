LIBCANVAS_WORKING_DIR='/tmp/libcanvas-gh-pages'
WORKING_DIRNAME="$(basename "$LIBCANVAS_WORKING_DIR")"

ORIGIN_PATH="$(dirname "$(dirname "$(realpath "$0")")")"
echo "$ORIGIN_PATH"

if [ -d "$LIBCANVAS_WORKING_DIR" ]; then
    pushd "$LIBCANVAS_WORKING_DIR"
    git fetch origin gh-pages
    git checkout -f gh-pages
    git reset --hard origin/gh-pages
else
    rm -fr "$LIBCANVAS_WORKING_DIR"
    cd "$(dirname "$LIBCANVAS_WORKING_DIR")"
    git clone git@github.com:/franko/elementary-plotlib -b gh-pages "$WORKING_DIRNAME"
    pushd "$WORKING_DIRNAME"
fi

git rm -frq .

cp -r "$ORIGIN_PATH/sphinx-doc/homepage/build/html/." .
mkdir manual
cp -r "$ORIGIN_PATH/sphinx-doc/manual/build/html/." manual
touch .nojekyll
git add --all

if [ -z "$(git status --porcelain)" ]; then
    echo "Nothing new to publish."
else
    git commit -m "Sphinx-doc update"
    read -p "Do you wish to push results to github (y/n) ? " yn
    case $yn in
        [Yy]* ) git push origin gh-pages;;
        * ) echo "No action done"; exit;;
    esac
fi
