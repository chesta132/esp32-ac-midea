import os

Import("env")

if os.path.exists(".env"):
    with open(".env") as f:
        for line in f:
            line = line.strip()
            # Abaikan baris kosong dan komentar
            if line and not line.startswith("#"):
                key, value = line.split("=", 1)
                
                # Bersihkan kutip bawaan dari file .env (jika ada)
                clean_val = value.strip('"\'')
                
                # Tambahkan macro ke C++ dengan escaping kutip yang benar
                env.Append(CPPDEFINES=[(key, f'\\"{clean_val}\\"')])