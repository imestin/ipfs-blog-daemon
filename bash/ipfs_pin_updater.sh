#!/bin/bash

# This will update the website, the database and the articles periodically (cron will call this script)

# Timestamp
echo $(date +"%Y.%m.%d. %H:%M:%S") >> ipfs_pin_updater.log

# Add website by IPNS name
/usr/local/bin/ipfs pin add -r /ipns/k2k4r8kgworvwegkpuirutkb9l515r9h3g0ar3k90bdd5sxyxjy861kq >> ipfs_p$
# Add database.json by IPNS name
/usr/local/bin/ipfs pin add -r /ipns/k2k4r8oid3l6x7ujkn727ziie9kd5u76q7wv3drqyjdesv9b9tudjd3r >> ipfs_p$
# Add articles folder by IPNS name
/usr/local/bin/ipfs pin add -r /ipns/k2k4r8jguqxqw3pk96718a7heewolatcoev7itv9zv8mgtcw4e6ki0zo >> ipfs_p$


