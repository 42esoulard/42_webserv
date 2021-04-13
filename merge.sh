git add -A .
echo "Commit message: "
read msg
echo "Name of your branch: "
read branch_name
git commit -m "$msg"
git checkout master
git pull origin master
git checkout $branch_name
git rebase master
git checkout master
git merge $branch_name
git push origin master
git branch -D $branch_name