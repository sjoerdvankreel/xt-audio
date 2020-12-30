namespace Xt
{
    public class ChannelInfo
    {
        public int Index { get; }
        public string Name { get; }
        public override string ToString() => Name;
        public ChannelInfo(int index, string name) => (Index, Name) = (index, name);
    }
}