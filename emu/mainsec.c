blst_scalar sk;
blst_scalar secret_keys_store[10];
blst_scalar sk_sign;
char public_keys_hex_store[960];
int keystore_size = 0;

__TZ_NONSECURE_ENTRY_FUNC
int get_keystore_size(){
        return keystore_size;
}

__TZ_NONSECURE_ENTRY_FUNC
void store_pk(char* public_key_hex){
        int cont = keystore_size - 1;
        for(int i = 0; i < 96; i++){
            public_keys_hex_store[i+96*cont] = public_key_hex[i];
        }
}

__TZ_NONSECURE_ENTRY_FUNC
void getkeys(char* public_keys_hex_store_ns){
        for(int i = 0; i < keystore_size*96; i++){
            public_keys_hex_store_ns[i] = public_keys_hex_store[i];
        }
}

__TZ_NONSECURE_ENTRY_FUNC
int pk_in_keystore(char * public_key_hex, int offset){

        int ret = 0;

        int c = 0;
        int cont = 0;

        for(int i = 0; i < keystore_size; i++){
            for(int x = 0; x < 96; x++){
                if(public_key_hex[x + offset] != public_keys_hex_store[x + cont]){
                    c = 1;
                    break;
                }
            }
            if (c == 0){
                sk_sign = secret_keys_store[i];
                break;
            } else {
                if((i+1) < keystore_size){
                    cont += 96;
                    c = 0;
                }else{
                    ret = -1;
                }
            }
        }
        return ret;
}

__TZ_NONSECURE_ENTRY_FUNC
void ikm_sk(char* info){
        // For security, IKM MUST be infeasible to guess, e.g., generated by a trusted
        // source of randomness. IKM MUST be at least 32 bytes long, but it MAY be longer.
        unsigned char ikm[32];
	const int random_number_len = 144;     
        uint8_t random_number[random_number_len];
        size_t olen = random_number_len;
        int ret;

        ret = spm_request_random_number_nse(random_number, random_number_len, &olen);
        
        ocrypto_sha256(ikm, random_number, random_number_len);
        
        //Secret key (256-bit scalar)
        blst_keygen(&sk, ikm, sizeof(ikm), info, sizeof(info));
        secret_keys_store[keystore_size] = sk;
        keystore_size++;
}

__TZ_NONSECURE_ENTRY_FUNC
void sk_to_pk(blst_p1* pk){
        blst_sk_to_pk_in_g1(pk, &sk);
}

__TZ_NONSECURE_ENTRY_FUNC
void sign_pk(blst_p2* sig, blst_p2* hash){
        blst_sign_pk_in_g1(sig, hash, &sk_sign);
}

__TZ_NONSECURE_ENTRY_FUNC
void reset(){
        memset(secret_keys_store, 0, sizeof(secret_keys_store));
        memset(public_keys_hex_store, 0, 960);
        keystore_size = 0;
}

void main(void)
{
	spm_config();
	spm_jump();
}