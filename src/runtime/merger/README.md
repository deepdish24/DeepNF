#######################################################

SETTING UP AND RUNNING MERGER

#######################################################

1. Setup docker container (from AWS instance, in /merger folder)
    => docker build -t=merger .
    => docker run -d -t -i --name merger merger:latest /bin/bash
    => docker exec -t -i merger /bin/bash
    => bash build.sh; ./merge

2. To copy files from AWS instance to container
    => docker cp . merger:/