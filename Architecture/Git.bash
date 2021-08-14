# 오픈소스 프로젝트에 참여할때를 가정. 
# Commit message, history 보기 
git log 
git log --oneline

# merge commit 제외 
git log --no-merges --oneline
git shortlog -sn --no-merges

# 특정 폴더를 기준으로 소스 수정내역 (commit) 리스트 확인 
git log --oneline --no-merges -- mnist/
  
# 특정시간, 특정 폴더 히스토리 확인 
git log --oneline --after=2020-01-01 --before=2020-06-30 -- mnist/  
  

# 옛날 커밋 정보 확인 
git log --oneline --reverse | head -5
git show -q 23933fc 


git config --global user.email "uphoo96@gmail.com" 
git config --global user.name lsh
# 오픈소스 개발 참여, 브랜치 설정
" 같은 폴더 다른 세상 "

  
git add main.py
git commit -m "add annoation at first"
git show 
git show | grep "diff"

  
# branch 생성
git checkout -b fix-mnist 

git push origin fix-mnist 
git remote -v
  

  
# remote 레포지토리, 본인 : origin , 개발 : upstream
git remote -v
git remote add upstream https://github.com/Taeung/pytorch-example.git
git remote add [레포 이름] [url] 
git remote remove [레포 이름]
  
  
# Rebase 가 필요한 상황 
# 히스토리가 갱신된 경우 
# 히스토리를 위에다 쌓아야 하지만 그러지 못한 경우 
# base 만 교체하는 작업 필요 
# 최신 내용을 가져와야한다. (upstream 에서 fetch 라는 명령어로 가져온다. )

# pull = fetch + merge 

# 1. 최신 내용 가져오기, 공식 upstream 저장소에서 최신 commit history 가져오기  
git fetch upstream master
git fetch [url] [브랜치 이름]

# 2. 최신 base 로 바뀐다. 
git rebase upstream/master

# 3. 중간의 히스토리가 바뀌었기 때문에 => force push 를 써야한다. 
git push --force origin fix-mnist

# (원상복구 1.) 수정한 내용을 잠시 저장하기, 완전 날려버리고 싶지 않은 경우  
git stash  
# 저장해둔 내용 복구 
git stash pop 

# (원상복구 2.) 이전 상태로 돌아가기, 완전 날려버리고 싶은 경우  
git checkout -- main.py

# add 한것 취소 
git add main.py
git reset 

# commit 삭제하기 
# HEAD~1 은 위에서 첫번째 내용을 삭제한다는 의미 
git reset --hard HEAD~1

# 삭제 후 가장 최신의 커밋 확인해보기 
git log --oneline -1

# -s 태그를 넣어서 저자 정보 넣기 (서명, 라이센스를 다 이해했다는 체크) 
git commit -s -m "add request modules"

# 실수로 작성한 내용, 커밋 후에 커밋 자체를 수정하기 
# 되감고 수정, 
git commit --amend 

# 중간의 커밋 내용을 수정하기 ?! 
다음시간. 

