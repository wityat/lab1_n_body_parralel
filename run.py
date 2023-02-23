import os
import pandas as pd
import time
import tqdm

n_threads = [1, 2, 4, 8, 16, 32]
# n_threads = [1, 2]
n_bodies = [100, 500, 1000, 2000]
# n_bodies = [100]
d = {}
for i in tqdm.tqdm(n_bodies):
    for n in n_threads:
        st = time.time()
        c = os.popen(
            f"""C:\Windows\system32\wsl.exe --distribution Ubuntu-20.04 --exec /bin/bash -c "cd '/mnt/c/Users/Витя Кенг/CLionProjects/untitled' && '/mnt/c/Users/Витя Кенг/CLionProjects/untitled/cmake-build-debug/untitled' input{i}.csv {n}"""
        ).read()
        res = time.time() - st
        try:
            d[i].append(float(c.replace("Time: ", "").replace(" seconds", "")))
        except Exception:
            d[i] = [float(c.replace("Time: ", "").replace(" seconds", ""))]
df = pd.DataFrame(d, index=n_threads)
print(df.to_markdown())
