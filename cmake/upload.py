#!/usr/bin/env python3

from glob import glob
from google.cloud import storage
from platform import uname
from sys import argv

import distro

client = storage.Client('arcas-326114')
bucket = client.get_bucket('chromium-dev')
sys = distro.id()
if not sys:
    sys = uname().system
prefix = f'{argv[1]}/{sys}'
for splat in ["*.deb", "*.rpm", "*.dmg", "ipfs-client-dev-*.tar.gz"]:
    for f in glob(splat):
        print("Uploading",f,f"to chromium-dev/{prefix}")
        blob = bucket.blob(f"{prefix}/{f}")
        blob.upload_from_filename(f)
