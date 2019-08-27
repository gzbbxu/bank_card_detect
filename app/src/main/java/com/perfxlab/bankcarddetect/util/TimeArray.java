package com.perfxlab.bankcarddetect.util;

public class TimeArray {
	private long[] arrTime = new long[50];
	
	class Index
	{
		private long index = 0;

		public void setIndex(long index) {
			this.index = index;
		}

		public int phyIndex(long idx)
		{
			return (int)(idx % arrTime.length);
		}
		public int inc()
		{
			return phyIndex(index++);
		}
		public long count()
		{
			return index;
		}
	}
	private Index index = new Index();
	
	public long count()
	{
		return index.count();
	}

	public void rest(){
		index.setIndex(0);
	}
	
	public void newTime()
	{
		arrTime[index.inc()] = System.currentTimeMillis();
	}

	public Index getIndex() {
		return index;
	}
	//public void clear() {
	//	index.index = 0;
	//}
	
	// 计算速率
	public double rate()
	{
		long indexBegin = index.count() - arrTime.length;
		if (indexBegin < 0)
			indexBegin = 0;
			
		if (index.count() < 2)
			return 0;
		else
			return 1000.0 * (index.count() - indexBegin) / (arrTime[index.phyIndex(index.count()-1)] - arrTime[index.phyIndex(indexBegin)]);
	}
}
