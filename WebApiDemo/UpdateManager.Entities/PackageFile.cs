using System;
using System.ComponentModel.DataAnnotations;

namespace UpdateManager.Entities
{
    public class PackageFile
    {
        #region Properties
        public int Id { get; set; }

        public int UpdatePackageId { get; set; }

        [Required]
        [StringLength(255)]
        public string FileName { get; set; }

        [Required]
        [StringLength(500)]
        public string FilePath { get; set; }

        [Required]
        public int FileSize { get; set; }

        [Required]
        public string Roles { get; set; }

        public DateTime CreationDate { get; set; }

        public string CreatedBy { get; set; }

        public virtual UpdatePackage UpdatePackage { get; set; }
        #endregion
    }
}
