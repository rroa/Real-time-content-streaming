using UpdateManager.Entities;

namespace UpdateManager.Web.ViewModels
{
    public class FileViewModel
    {
        public string Name { get; set; }

        public UpdatePackagePriority Type { get; set; }

        public string Roles { get; set; }

        public int Size { get; set; }
    }
}