seq = []
pow_2 = 1

while pow_2 <= 10**4:
	count = pow_2 * 1100 + 4 + (pow_2 + 7) // 8
	seq.append(count)
	if(pow_2 != 1):
		pow_tmp = 3 * pow_2 // 2
		count = pow_tmp * 1100 + 4 + (pow_tmp + 7) // 8
		seq.append(count)

	pow_2 *= 2
print(seq)