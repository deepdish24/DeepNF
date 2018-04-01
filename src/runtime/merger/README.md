#######################################################

SETTING UP AND RUNNING MERGER

#######################################################

1. Setup docker container (from AWS instance, in /merger folder)
    => docker build -t=merger .
    => docker run -d -t -i --user $(id -u) --name merger merge:latest /bin/bash

2. Run bash as "appuser"
    => docker exec -t -i --user $(id -u) merger /bin/bash

3. Install nlohmann/json
    => brew tap nlohmann/json; brew install nlohmann_json