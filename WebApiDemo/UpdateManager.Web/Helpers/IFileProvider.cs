using System.IO;

namespace UpdateManager.Web.Helpers
{
    public interface IFileProvider
    {
        bool Exists(string name);
        FileStream Open(string name);
        long GetLength(string name);
    }
}