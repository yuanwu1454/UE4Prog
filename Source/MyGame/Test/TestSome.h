void TestString()
{
	FString Str1 = "Hello";
	FString Str2 = TEXT("Hello");
	const char* CStr = "Hello C String";
	FString Str3 = CStr;

	{
		int32 a = 1;
		uint32 d = 2;
		double f= 3.f;
		char x = 'c';
		int32* p = &a;
		
		// Str2 之所以要解引用，是因为支持的 const TCHAR* 类型
		UE_LOG(LogTemp, Log, TEXT("Str1 %s %d %u %f %c%p"), *Str2, a, d, f, x, p);

		const char* s = "asdf";
		// TEXT 格式化字符串是TCHAR类型
		// UE_LOG(LogTemp, Log, TEXT("Str1 %s"), s);
		UE_LOG(LogTemp, Log, TEXT("Str1 %s"), UTF8_TO_TCHAR(s));
		UE_LOG(LogTemp, Log, TEXT("Str1 %s"), *FString(UTF8_TO_TCHAR(s)));
		
		const TCHAR* ss = TEXT("zxvxcv");
		UE_LOG(LogTemp, Log, TEXT("Str1 %s"), ss);
		
	}
}