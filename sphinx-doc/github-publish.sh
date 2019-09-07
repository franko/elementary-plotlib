LIBCANVAS_WORKING_DIR='/tmp/libcanvas-gh-pages'
WORKING_DIRNAME="$(basename "$LIBCANVAS_WORKING_DIR")"

ORIGIN_PATH="$(dirname "$(dirname "$(realpath "$0")")")"
echo "$ORIGIN_PATH"

rm -fr "$LIBCANVAS_WORKING_DIR"
pushd "$(dirname "$LIBCANVAS_WORKING_DIR")"
git clone git@github.com:/franko/libcanvas -b gh-pages "$WORKING_DIRNAME"
pushd "$WORKING_DIRNAME"

git rm -frq .

cp -r "$ORIGIN_PATH/sphinx-doc/build/html/." .
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
