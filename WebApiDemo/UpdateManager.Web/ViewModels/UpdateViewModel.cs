using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;
using UpdateManager.Entities;

namespace UpdateManager.Web.ViewModels
{
    public class UpdateViewModel
    {
        [Required]
        [StringLength(255)]
        public string Name { get; set; }

        [Required]
        [StringLength(255)]
        public string Description { get; set; }

        public UpdateType Type { get; set; }
        
        public HttpPostedFileBase ChangeLog { get; set; }
    }
}