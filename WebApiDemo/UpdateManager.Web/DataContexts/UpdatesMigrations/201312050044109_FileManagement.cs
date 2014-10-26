namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class FileManagement : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "UpdateManager.PackageFiles",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        UpdatePackageId = c.Int(nullable: false),
                        FileName = c.String(),
                        FilePath = c.String(),
                        FileSize = c.Int(nullable: false),
                        CreationDate = c.DateTime(nullable: false),
                        CreatedBy = c.String(),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "UpdateManager.UpdatePackages",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        UpdateId = c.Int(nullable: false),
                        CreationDate = c.DateTime(nullable: false),
                        UpdateDate = c.DateTime(nullable: false),
                        CreatedBy = c.String(),
                    })
                .PrimaryKey(t => t.Id);
            
            AlterColumn("UpdateManager.Updates", "Name", c => c.String(maxLength: 200));
            AlterColumn("UpdateManager.Updates", "Description", c => c.String(maxLength: 500));
        }
        
        public override void Down()
        {
            AlterColumn("UpdateManager.Updates", "Description", c => c.String());
            AlterColumn("UpdateManager.Updates", "Name", c => c.String());
            DropTable("UpdateManager.UpdatePackages");
            DropTable("UpdateManager.PackageFiles");
        }
    }
}
