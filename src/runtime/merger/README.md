#######################################################

SETTING UP AND RUNNING MERGER

#######################################################

1. Setup docker container (from AWS instance, in /merger folder)
    => docker build -t=merger .
    => docker run -d -t -i --name merger merger:latest /bin/bash

2. Run bash as root
    => docker exec -t -i --user root merger /bin/bash
    => chmod 777 /home/; chmod 777 /

2. Run bash as "appuser" and install nlohmann/json
    => docker exec -t -i --user $(id -u) merger /bin/bash
    => brew tap nlohmann/json; brew install nlohmann_json

4. Copy merger files into container, build and run
    => docker cp . merger:/
    => docker exec -t -i --user root merger /bin/bash
    => bash build.sh; ./merge [path to action table]